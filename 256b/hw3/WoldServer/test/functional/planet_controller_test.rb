require 'test_helper'

class PlanetControllerTest < ActionController::TestCase
  test "should get post" do
    get :post
    assert_response :success
  end

  test "should get get" do
    get :get
    assert_response :success
  end

end
